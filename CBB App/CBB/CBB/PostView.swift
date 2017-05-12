//
//  PostView.swift
//  CBB
//
//  Created by Erich Chu on 4/22/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import SwiftyJSON

@IBDesignable class PostView: UIView {
    
    var view: UIView!
    var user_id = ""
    var user_name = ""
    var owner_id = ""
    var post_id = ""
    var timestamp = ""
    var postDate = ""
    var postTime = ""
    var title = ""
    var desc = ""
    var poster_name = ""
    var community_name = ""
    var community_oid = ""
    var controller = PostsViewController(nibName: "PostsViewController", bundle: nil)
    
    @IBOutlet weak var postTitle: UILabel!
    @IBOutlet weak var postDesc: UITextView!
    
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setup()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        setup()
    }
    
    func setup() {
        view = loadViewFromNib()
        view.frame = bounds
        view.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        view.layer.borderWidth = 5
        view.layer.borderColor = UIColor(white: 0, alpha: 0.5).cgColor
        
        UIGraphicsBeginImageContext(self.view.frame.size)
        #imageLiteral(resourceName: "post-it2").draw(in: self.view.bounds)
        var image: UIImage = UIGraphicsGetImageFromCurrentImageContext()!
        UIGraphicsEndImageContext()
        
        view.backgroundColor = UIColor(patternImage: image)

        addSubview(view)
    }
    
    func loadViewFromNib() -> UIView {
        let bundle = Bundle(for:type(of: self))
        let nib = UINib(nibName: "PostView", bundle: bundle)
        let view = nib.instantiate(withOwner: self, options: nil)[0] as! UIView
        
        return view
    }
    
    @IBAction func loadPostFullView(_ sender: UIButton) {
        if (sender.titleLabel?.text == "View") {
            let revealViewController:SWRevealViewController = self.controller.revealViewController()
            let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
            let desController = mainStoryboard.instantiateViewController(withIdentifier: "ViewPostViewController") as! ViewPostViewController
            
            desController.user_id = self.user_id
            desController.name = self.poster_name
            desController.pTitle = self.title
            desController.pDesc = self.desc
            desController.postdate = self.postDate
            desController.posttime = self.postTime
            
            desController.owner_id = self.owner_id
            desController.post_id = self.post_id
            desController.community_name = self.community_name
            desController.community_oid = self.community_oid
            
            let newFrontViewController = UINavigationController.init(rootViewController:desController)
            revealViewController.pushFrontViewController(newFrontViewController, animated: true)
        }
    }
    
    @IBAction func viewPostComments(_ sender: UIButton) {
        if (sender.titleLabel?.text == "Comment") {
            let revealViewController:SWRevealViewController = self.controller.revealViewController()
            let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
            let desController = mainStoryboard.instantiateViewController(withIdentifier: "CommentsViewController") as! CommentsViewController
            
            desController.user_name = self.user_name
            desController.user_id = self.user_id
            desController.post_id = self.post_id
            desController.post_name = self.title
            desController.poster_name = self.poster_name
            desController.post_title = self.title
            desController.post_desc = self.desc
            desController.post_date = self.postDate
            desController.post_time = self.postTime
            desController.post_oid = self.owner_id
            desController.community_name = self.community_name
            desController.community_oid = self.community_oid
            
            let newFrontViewController = UINavigationController.init(rootViewController:desController)
            revealViewController.pushFrontViewController(newFrontViewController, animated: true)
        }
    }
    
}
